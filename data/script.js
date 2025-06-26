function updateStatus() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/api/status', true);
  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4 && xhr.status == 200) {
      var data = JSON.parse(xhr.responseText);
      document.getElementById('status').innerHTML =
        'LED : ' + data.led + '<br>' +
        'Température : ' + data.temperature + ' °C<br>' +
        'Humidité : ' + data.humidity + ' %';
    }
  };
  xhr.send();
}
function sendCommand(cmd) {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/led/' + cmd, true);
  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4 && xhr.status == 200) { updateStatus(); }
  };
  xhr.send();
}
setInterval(updateStatus, 5000);
window.onload = updateStatus;