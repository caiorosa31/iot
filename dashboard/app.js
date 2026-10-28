const $ = (id) => document.getElementById(id);
let client = null;
let prefix = "";
let cargaEstado = null;

// restaura última configuração usada (conveniência local, por navegador)
try {
  const saved = JSON.parse(localStorage.getItem('dash-energia-conn') || '{}');
  if (saved.host) $('host').value = saved.host;
  if (saved.port) $('port').value = saved.port;
  if (saved.prefix) $('prefix').value = saved.prefix;
} catch(e){}

function log(dir, topic, payload){
  const el = $('log');
  const time = new Date().toLocaleTimeString('pt-BR', {hour12:false});
  const row = document.createElement('div');
  let cls = dir === 'pub' ? 'pub' : dir === 'sub' ? 'sub' : dir === 'crit' ? 'crit' : 'sys';
  let arrow = dir === 'pub' ? '↑ enviado' : dir === 'sub' ? '↓ recebido' : '·';
  row.className = cls;
  row.innerHTML = `<span class="t">${time}</span>${arrow}  ${topic ? topic + '  ' : ''}${payload}`;
  el.appendChild(row);
  el.scrollTop = el.scrollHeight;
  while (el.children.length > 80) el.removeChild(el.firstChild);
}

function setConnected(on){
  $('badge-mqtt').classList.toggle('on', on);
  $('btn-connect').disabled = on;
  $('btn-disconnect').disabled = !on;
  $('btn-on').disabled = !on;
  $('btn-off').disabled = !on;
  $('host').disabled = on;
  $('port').disabled = on;
  $('prefix').disabled = on;
}

function setLoadUI(on){
  cargaEstado = on;
  $('led').classList.toggle('on', on);
  $('load-text').textContent = 'CARGA: ' + (on ? 'ON' : 'OFF');
}

function setStateUI(estado){
  const banner = $('state-banner');
  const cls = estado === 'ATENCAO' ? 'atencao' : estado === 'CRITICO' ? 'critico' : 'normal';
  banner.className = 'state-banner ' + cls;
  $('state-text').textContent = estado === 'ATENCAO' ? 'ATENÇÃO' : estado;
}

$('btn-connect').addEventListener('click', () => {
  const host = $('host').value.trim();
  const port = $('port').value.trim();
  prefix = $('prefix').value.trim().replace(/\/$/, '');

  if (!host || !port || !prefix) { log('sys', null, 'preencha host, porta e prefixo'); return; }

  localStorage.setItem('dash-energia-conn', JSON.stringify({host, port, prefix}));

  const url = `ws://${host}:${port}`;
  log('sys', null, `conectando em ${url}...`);
  client = mqtt.connect(url, { connectTimeout: 6000, reconnectPeriod: 0 });

  client.on('connect', () => {
    setConnected(true);
    log('sys', null, 'conectado ao broker');
    [`${prefix}/telemetria/tensao`, `${prefix}/telemetria/corrente`, `${prefix}/telemetria/potencia`,
     `${prefix}/alerta`, `${prefix}/confirmacao/carga`, `${prefix}/status`].forEach(t => client.subscribe(t));
  });

  client.on('message', (topic, payload) => {
    const msg = payload.toString();
    log('sub', topic, msg);

    if (topic.endsWith('/telemetria/tensao')) $('v-tensao').textContent = Number(msg).toFixed(2);
    else if (topic.endsWith('/telemetria/corrente')) $('v-corrente').textContent = Number(msg).toFixed(3);
    else if (topic.endsWith('/telemetria/potencia')) $('v-potencia').textContent = Number(msg).toFixed(3);
    else if (topic.endsWith('/alerta')) setStateUI(msg);
    else if (topic.endsWith('/confirmacao/carga')) setLoadUI(msg === 'ON');
  });

  client.on('error', (err) => log('crit', null, 'erro: ' + err.message));
  client.on('close', () => { setConnected(false); log('sys', null, 'desconectado'); });
});

$('btn-disconnect').addEventListener('click', () => {
  if (client) client.end();
  setConnected(false);
});

$('btn-on').addEventListener('click', () => {
  if (client) { client.publish(`${prefix}/comando/carga`, 'ON'); log('pub', `${prefix}/comando/carga`, 'ON'); }
});
$('btn-off').addEventListener('click', () => {
  if (client) { client.publish(`${prefix}/comando/carga`, 'OFF'); log('pub', `${prefix}/comando/carga`, 'OFF'); }
});
