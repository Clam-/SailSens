const LOG = [];
export function log(s) {
  var elem = document.getElementById("logbox");
  if (LOG.push(s) > 10) { LOG.shift(); }
  elem.innerText = LOG.join("\n");
  elem.scrollTop = elem.scrollHeight;
}
