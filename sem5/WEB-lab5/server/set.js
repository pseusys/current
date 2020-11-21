const substractor = require('./substractor');

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
    const settings = substractor.write_settings(JSON.parse(req.body.settings));
    if (!!settings) res.json(settings).end();
    else return res.status(500).end("Настройки не изменены.");
  });
}
