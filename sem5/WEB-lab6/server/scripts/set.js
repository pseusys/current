const substractor = require('./substractor');
const trader = require('./trader');

module.exports.configure = function (server) {
  /** Accepts:
   * @param settings
   */
  server.get("/settings", (req, res) => {
    const settings = substractor.read_settings();
    if (!!settings) res.json(settings).end();
    else return res.status(500).end("Настройки не найдены.");
  });

  /** Accepts:
   * @param settings
   */
  server.put("/settings", (req, res) => {
    const set = JSON.parse(req.body.settings);
    if (set.tradeStart > set.tradeEnd) return res.status(500).end("Начало торгов не может быть позже конца.");
    const settings = substractor.write_settings(set);
    trader.trade(set);
    if (!!settings) res.json(settings).end();
    else return res.status(500).end("Настройки не изменены.");
  });
}
