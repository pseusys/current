// @flow

const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;

const population = require('./population');
const log = require('./logger');

module.exports.configure = function (server) {
    server.use(require('cookie-parser')());
    server.use(require('express-session')({
        secret: 'secret',
        resave: true,
        saveUninitialized: true
    }));
    server.use(passport.initialize());
    server.use(passport.session());



    passport.serializeUser(function(user, done) {
        done(null, user);
    });

    passport.deserializeUser(function(user, done) {
        done(null, user);
    });

    passport.use("login-local", new LocalStrategy(
        function(username, password, done) {
            log.logger.info("Logging in user: " + username);
            const user = population.get_user(username, password);
            if (user) return done(null, user);
            else return done("Пользователь не найден или пароль не верен.", null);
        }
    ));

    passport.use("signin-local", new LocalStrategy(
        function(username, password, done) {
            log.logger.info("Signing in user: " + username);
            const user = population.set_user(username, password);
            if (user) return done(null, user);
            else return done("Пользователь уже существует.", null);
        }
    ));


    /** Accepts:
     * @param username = user name
     * @param password = password
     */
    server.post("/login", (req, res) => {
        const callback = function(err, user, info) {
            if (err) return res.status(500).end(err.toString());
            req.logIn(user, function(err) {
                if (err) return res.status(500).end("Не удалось войти.");
                return res.end();
            });
        };
        passport.authenticate('login-local', {}, callback)(req, res);
    });

    server.delete("/login", (req, res) => {
        req.logout();
        res.end();
    });



    /** Accepts:
     * @param username = user name
     * @param password = password
     */
    server.post("/user", (req, res) => {
        const callback = function(err, user, info) {
            if (err) return res.status(500).end(err.toString());
            req.logIn(user, function(err) {
                if (err) return res.status(500).end("Не удалось зарегистрироваться.");
                return res.end();
            });
        };
        passport.authenticate('signin-local', {}, callback)(req, res);
    });



    server.get("/user", (req, res) => {
        res.json(req.user).end();
    });

    server.delete("/user", (req, res) => {
        population.delete_user(req.user);
        req.logout();
        res.end();
    });



    server.post("/users", (req, res) => {
        const username = req.body.username ? req.body.username : req.user.name;
        const user = population.get_user(username);
        if (user) {
            if (req.body.part) user.part = req.body.part === "true";
            if (req.body.money) user.money = Number.parseInt(req.body.money);
            population.edit_user(user);
        }
        res.end();
    });
}