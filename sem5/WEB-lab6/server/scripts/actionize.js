const substractor = require('./substractor');

module.exports.configure = function (server) {
  server.get("/actions", (req, res) => {
    return res.json(substractor.get_actions()).end();
  });


  /** Accepts:
   * @param action = action
   */
  server.post("/action", (req, res) => {
    const action = substractor.add_action(JSON.parse(req.body.action));
    if (!!action) res.json(action).end();
    else return res.status(500).end("Акция не добавлена.");
  });

  /** Accepts:
   * @param name = action name
   */
  server.delete("/action", (req, res) => {
    substractor.delete_action(req.query.name);
    return res.end();
  });

  /** Accepts:
   * @param name = action name
   */
  server.get("/action", (req, res) => {
    const action = substractor.get_action(req.query.name);
    if (!!action) res.json(action).end();
    else return res.status(500).end("Акция не найдена.");
  });

  /** Accepts:
   * @param actions = action to change
   */
  server.put("/action", (req, res) => {
    const result = substractor.edit_action(JSON.parse(req.body.action));
    if (!!result) res.json(result).end();
    else return res.status(500).end("Акция не изменена.");
  });
}
