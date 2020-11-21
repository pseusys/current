const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;

const substractor = require('./substractor');

module.exports.configure = function (server) {
  server.use(require('cookie-parser')());
  server.use(require('express-session')({
    secret: 'secret',
    resave: true,
    saveUninitialized: true
  }));
  server.use(passport.initialize());
  server.use(passport.session());


  passport.serializeUser(function(user, done) { done(null, user); });
  passport.deserializeUser(function(user, done) { done(null, user); });

  passport.use("login-local", new LocalStrategy({},
    function(username, password, done) {
      console.log("Logging in user: " + username);
      const user = substractor.check_broker(username, password);
      if (!!user) return done(null, user);
      else return done("Пользователь не найден или пароль не верен.", null);
    }
  ));

  passport.use("signin-local", new LocalStrategy({},
    function(username, password, done) {
      console.log("Signing in user: " + username);
      const user = substractor.add_broker(username, password);
      if (!!user) return done(null, user);
      else return done("Пользователь уже существует.", null);
    }
  ));


  /** Accepts:
   * @param username = user username
   * @param password = password
   */
  server.post("/login", (req, res) => {
    const callback = function(err, user) {
      if (err) return res.status(500).end(err.toString());
      req.logIn(user, function(err) {
        if (err) {
          console.log("Error logging in:", err);
          return res.status(500).end("Не удалось войти.");
        }
        return res.json(user).end();
      });
    };
    passport.authenticate('login-local', {}, callback)(req, res);
  });

  server.delete("/login", (req, res) => {
    req.logout();
    res.end();
  });


  /** Accepts:
   * @param username = user username
   * @param password = password
   */
  server.post("/signin", (req, res) => {
    const callback = function(err, user) {
      if (err) return res.status(500).end(err.toString());
      req.logIn(user, function(err) {
        if (err) {
          console.log("Error signing in", user, ":", err);
          return res.status(500).end("Не удалось зарегистрироваться.");
        }
        return res.json(user).end();
      });
    };
    passport.authenticate('signin-local', {}, callback)(req, res);
  });

  /** Accepts:
   * @param user = username
   */
  server.delete("/signin", (req, res) => {
    substractor.delete_broker(req.query.user);
    req.logout();
    res.end();
  });

  /** Accepts:
   * @param user = username
   */
  server.get("/user", (req, res) => {
    const result = substractor.get_broker(req.query.user);
    if (!!result) res.json(result).end();
    else return res.status(500).end("Пользователь не найден.");
  });

  /** Accepts:
   * @param user = user to change
   */
  server.post("/user", (req, res) => {
    const result = substractor.edit_broker(JSON.parse(req.body.user));
    if (!!result) res.json(result).end();
    else return res.status(500).end("Пользователь не изменён.");
  });


  server.get("/users", (req, res) => {
    return res.json(substractor.get_brokers()).end();
  });
}
