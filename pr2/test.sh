#!/usr/bin/env sh

cd /var/www/html/drupal || exit 1
echo "Note: include your SimpleTest module (in browser)"
sudo php ./core/scripts/run-tests.sh --verbose --module practice
