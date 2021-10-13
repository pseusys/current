<?php
/**
 * @file
 * Contains \Drupal\practice\Tests\MyEntityTest.
 */

namespace Drupal\practice\Tests;


use Drupal\practice\Entity\MyEntity;
use Drupal\Tests\file\Kernel\FileManagedUnitTestBase;

/**
 * @coversDefaultClass \Drupal\practice\Entity\MyEntity
 * @group practice
 */
class MyEntityTest extends FileManagedUnitTestBase {
  public static $modules = ['practice', 'file', 'system'];

  protected function setUp() {
    parent::setUp();
    $this->installEntitySchema('my_entity');
  }

  /**
   * @throws \Drupal\Core\Entity\EntityStorageException
   */
  public function testSaveAdvertiser() {
    $sample_text = 'Sample Text';

    // Create an entity.
    $entity = MyEntity::create();

    $entity->set('name', $sample_text);

    // Save it.
    $entity->save();

    // Get the id.
    $id = $entity->id();

    // Load the saved entity.
    $saved_entity = MyEntity::load($id);

    $entity_name = $saved_entity->get('name')->value;

    // Check label.
    $this->assertEquals($sample_text, $entity_name, 'ERROR: ' . $entity_name . ' != ' . $sample_text);
  }
}
