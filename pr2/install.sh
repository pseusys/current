#!/usr/bin/env sh

# Based on following article: https://websiteforstudents.com/install-drupal-cms-on-ubuntu-18-04-lts-beta-with-nginx-mariadb-and-php-7-1-fpm-support/

# First parameter: Drupal database name
# Second parameter: Drupal user name
# Third parameter: Drupal user password

if [ $# != 3 ]
then echo "Wrong arguments count!" && exit 1
else printf "Considered following:\nDrupal database name: %s\nDrupal user name: %s\nDrupal user password: %s\n", "$1", "$2", "$3"
fi

sudo apt-get update

# Installing and checking NGINX
sudo apt-get -y install nginx
systemctl start nginx

if systemctl status nginx --no-pager > /dev/null
then echo "NGINX running"
else echo "Problem launching NGINX" && exit 1
fi

# Installing and checking MariaDB
sudo apt-get -y install mariadb-server mariadb-client
systemctl start mariadb.service

if systemctl status mariadb.service --no-pager > /dev/null
then echo "MariaDB running"
else echo "Problem launching MariaDB" && exit 1
fi

# Installing and checking PHP7.1 and PHP-FPM
sudo apt-get -y install software-properties-common
sudo add-apt-repository ppa:ondrej/php
sudo apt update
sudo apt-get -y install php7.1-fpm php7.1-common php7.1-mbstring php7.1-xmlrpc php7.1-soap php7.1-gd php7.1-xml php7.1-intl php7.1-mysql php7.1-cli php7.1-mcrypt php7.1-tidy php7.1-recode php7.1-zip php7.1-curl

if systemctl status php7.1-fpm.service --no-pager > /dev/null
then echo "PHP-FPM running"
else echo "Problem launching PHP-FPM" && exit 1
fi

# Creating Drupal user and db in MariaDB
mysql -u root -p -Bse "CREATE DATABASE ${"$1"};CREATE USER '${"$2"}'@'localhost' IDENTIFIED BY '${"$3"}';GRANT ALL ON ${"$1"}.* TO '${"$2"}'@'localhost' IDENTIFIED BY '${"$3"}' WITH GRANT OPTION;FLUSH PRIVILEGES;EXIT;"

# Downloading Drupal
cd /tmp && wget https://ftp.drupal.org/files/projects/drupal-8.3.7.tar.gz
sudo tar -zxvf drupal*.gz -C /var/www/html
sudo mv /var/www/html/drupal-8.3.7/ /var/www/html/drupal/
sudo chown -R www-data:www-data /var/www/html/
sudo chmod -R 755 /var/www/html/

# Loading NGINX configuration
cat ./config/config > /etc/nginx/sites-available/drupal
sudo ln -s /etc/nginx/sites-available/drupal /etc/nginx/sites-enabled/

# Applying configuration and restarting system
systemctl restart nginx.service
systemctl restart php7.1-fpm.service

if systemctl status nginx --no-pager > /dev/null && systemctl status php7.1-fpm.service --no-pager > /dev/null
then echo "System running"
else echo "Problem launching system" && exit 1
fi

cd /var/www/html/drupal/ || return 1
composer update

echo "Enjoy your drupal site on http://127.0.0.1:80"
