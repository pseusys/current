const gulp = require('gulp4');
const less = require('gulp-less');
const rename = require('gulp-rename');
const cleanCSS = require('gulp-clean-css');
const babel = require('gulp-babel');
const concat = require('gulp-concat');
const uglify = require('gulp-uglify');
const pug = require('gulp-pug');

const clean = () => require('del')(['src']);

function styles () {
    return gulp.src("./public-bravo/styles/custom.less")
        .pipe(less())
        .pipe(cleanCSS())
        .pipe(rename({
            basename: "main"
        }))
        .pipe(gulp.dest("./src/"));
}

function scripts () {
    return gulp.src(["./public-bravo/scripts/*.js", "./public-bravo/pagescripts/*.js"])
        .pipe(babel())
        .pipe(concat("main.js"))
        .pipe(uglify())
        .pipe(rename({
            basename: "main",
            suffix: ".min"
        }))
        .pipe(gulp.dest("./src/"));
}

function views() {
    return gulp.src("./views-bravo/*.pug")
        .pipe(pug({
            client: true
        }))
        .pipe(babel())
        .pipe(uglify())
        .pipe(gulp.dest("./src/"));
}

function images() {
    return gulp.src(["./public-bravo/covers/*", "./public-bravo/images/*"])
        .pipe(gulp.dest("./src/images/"));
}

gulp.task("default", gulp.series(clean, styles, scripts, views, images));
