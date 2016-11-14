'use strict';

import gulp from 'gulp';
import gulpLoadPlugins from 'gulp-load-plugins';
import lazypipe from 'lazypipe';
import nodemon from 'nodemon';

var plugins = gulpLoadPlugins();

var onServerLog = function(log) {
  console.log(plugins.util.colors.white('[') +
      plugins.util.colors.yellow('nodemon') +
      plugins.util.colors.white('] ') +
      log.message);
}

var onError = function(err) {
  plugins.util.beep();
}

let lint = lazypipe()
    .pipe(plugins.jshint, '.jshintrc')
    .pipe(plugins.jshint.reporter, 'jshint-stylish');

gulp.task('default', ['start']);

gulp.task('start', ['watch'], () => {
    nodemon('-w').on('log', onServerLog)
        .on('crash', onError)
        .on('exit', onError);
});

gulp.task('watch', () => {
    plugins.watch('*.js')
        .pipe(plugins.plumber())
        .pipe(lint())
        .pipe(plugins.livereload());
});
