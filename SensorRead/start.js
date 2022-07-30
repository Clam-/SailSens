import * as log from './modules/logger.js'
import * as hc from './modules/HCsetup.js'
import * as poll from './modules/poller.js'

var charts = null;

function go(isClick) {
  if (charts === null) { charts = hc.setup(); }
  if (!poll.isRunning())
    poll.datastart(isClick, charts);
}

window.addEventListener('load', () => {
  document.getElementById("mainsheet").addEventListener("click", () => { go(true); })
  go(false);
});
