let ws = null;
let reconnectionAttempts = 0;
const maxReconnectionAttempts = 5;

function initWebSocket() {
  ws = new WebSocket('ws://' + window.location.hostname + '/ws');
  
  ws.onopen = function() {
    document.getElementById('wsStatus').textContent = 'Connecté';
    document.getElementById('wsStatus').classList.add('connected');
    document.getElementById('wsStatus').classList.remove('disconnected');
    document.getElementById('connexion').textContent = 'Connecté';
    document.getElementById('connexion').classList.add('on');
    document.getElementById('connexion').classList.remove('off');
    reconnectionAttempts = 0;
  };

  ws.onmessage = function(event) {
    let data = JSON.parse(event.data);
    if (data.type === 'etat') {
      // Mise à jour capteur
      document.getElementById('etatCapteur').textContent = data.capteur ? 'Actif' : 'Inactif';
      document.getElementById('etatCapteur').classList.toggle('on', data.capteur);
      document.getElementById('etatCapteur').classList.toggle('off', !data.capteur);
      
      // Mise à jour LED
      document.getElementById('etatLED').textContent = data.led ? 'Allumée' : 'Éteinte';
      document.getElementById('etatLED').classList.toggle('on', data.led);
      document.getElementById('etatLED').classList.toggle('off', !data.led);
      
      // Mise à jour mode
      document.getElementById('mode').textContent = data.mode.charAt(0).toUpperCase() + data.mode.slice(1);
      document.getElementById('mode').classList.toggle('auto', data.mode === 'automatique');
      document.getElementById('mode').classList.toggle('manual', data.mode === 'manuel');
      document.getElementById('nouveauMode').textContent = data.mode === 'automatique' ? 'manuel' : 'automatique';
    }
  };

  ws.onclose = function() {
    document.getElementById('wsStatus').textContent = 'Déconnecté';
    document.getElementById('wsStatus').classList.add('disconnected');
    document.getElementById('wsStatus').classList.remove('connected');
    document.getElementById('connexion').textContent = 'Déconnecté';
    document.getElementById('connexion').classList.add('off');
    document.getElementById('connexion').classList.remove('on');
    
    if (reconnectionAttempts < maxReconnectionAttempts) {
      reconnectionAttempts++;
      console.log(`Tentative de reconnexion ${reconnectionAttempts}/${maxReconnectionAttempts}...`);
      setTimeout(initWebSocket, 2000);
    } else {
      console.error('Échec des tentatives de reconnexion.');
    }
  };

  ws.onerror = function(error) {
    console.error('Erreur WebSocket :', error);
  };
}

function toggleLED() {
  if (ws && ws.readyState === WebSocket.OPEN) {
    let message = { 
      type: 'commande', 
      led: !document.getElementById('etatLED').textContent.includes('Allumée') 
    };
    ws.send(JSON.stringify(message));
  } else {
    alert('Erreur : Aucune connexion WebSocket active.');
  }
}

function toggleMode() {
  if (ws && ws.readyState === WebSocket.OPEN) {
    let message = { 
      type: 'mode', 
      mode: document.getElementById('mode').textContent.toLowerCase() === 'automatique' ? 'manuel' : 'automatique' 
    };
    ws.send(JSON.stringify(message));
  } else {
    alert('Erreur : Aucune connexion WebSocket active.');
  }
}

// Initialisation WebSocket au chargement de la page
initWebSocket();