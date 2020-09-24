module.exports.configure = function (server) {
    const bodyParser = require('body-parser');
    const passport = require('passport');
    const LocalStrategy = require('passport-local').Strategy;

    server.use(require('cookie-parser')());
    server.use(bodyParser.urlencoded({extended: true}));
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

    passport.use("local", new LocalStrategy(
        function(username, password, done) {
            console.log("Authing")
            return done(null, {name: username, password: password});
        }
    ));



    server.post('/login', (req, res, next) => {
        passport.authenticate('local', {}, function(err, user, info) {
            if (err) return res.res.status(500).end(err.toString());
            req.logIn(user, function(err) {
                if (err) return res.res.status(500).end(err.toString());
                return res.end();
            });
        })(req, res, next);
    });

    server.get('/logout', function(req, res){
        req.logout();
        res.end();
    });
}