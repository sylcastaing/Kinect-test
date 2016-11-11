'use strict';

import gulp from 'gulp';
import gulpLoadPlugins from 'gulp-load-plugins';
import lazypipe from 'lazypipe';
import nodemon from 'nodemon';

var plugins = gulpLoadPlugins();

let lint = lazypipe()
    .pipe(plugins.jshint, '.jshintrc')
    .pipe(plugins.jshint.reporter, 'jshint-stylish');

gulp.task('default', ['start']);

gulp.task('start', ['watch'], () => {
    nodemon('-w');
});

gulp.task('watch', () => {
    plugins.watch('*.js')
        .pipe(plugins.plumber())
        .pipe(lint())
        .pipe(plugins.livereload());
});