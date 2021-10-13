<?php
/**
 * @file
 * Contains \Drupal\practice\Controller\PracticeController.
 * ^ Пишется по следующему типу:
 *  - \Drupal - это указывает что данный файл относится к ядру Drupal, ведь
 *    теперь там еще есть Symfony.
 *  - practice - название модуля.
 *  - Controller - тип файла. Папка src опускается всегда.
 *  - PracticeController - название нашего класса.
 */

/**
 * Пространство имен нашего контроллера. Обратите внимание что оно схоже с тем
 * что описано выше, только опускается название нашего класса.
 */
namespace Drupal\practice\Controller;

/**
 * Используем друпальный класс ControllerBase. Мы будем от него наследоваться,
 * а он за нас сделает все обязательные вещи которые присущи всем контроллерам.
 */
use Drupal\Core\Controller\ControllerBase;

/**
 * Объявляем наш класс-контроллер.
 */
class PracticeController extends ControllerBase {

  /**
   * В Drupal 8 очень многое строится на renderable arrays и при отдаче
   * из данной функции содержимого для страницы, мы также должны вернуть
   * массив который спокойно пройдет через drupal_render().
   */
  public function helloWorld () {
    $output = array();

    $output['#title'] = 'HelloWorld module title';

    $output['#markup'] = 'Hello world!';

    return $output;
  }
}
