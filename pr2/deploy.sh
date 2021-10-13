#!/usr/bin/env sh

sudo mkdir -p /var/www/html/drupal/modules/practice/
sudo rm -rf /var/www/html/drupal/modules/practice/*
sudo cp -R ./sources/* /var/www/html/drupal/modules/practice/
