<?php
/**
 * @file
 * Contains \Drupal\practice\Entity\MyEntity.
 */

namespace Drupal\practice\Entity;

use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Field\BaseFieldDefinition;

/**
 * Defines the MyEntity.
 *
 * @ingroup my_entity
 *
 * @ContentEntityType(
 *   id = "my_entity",
 *   label = @Translation("MyEntity"),
 *   base_table = "my_entity",
 *   entity_keys = {
 *     "id" = "id",
 *     "name" = "name",
 *   },
 * )
 */

class MyEntity extends ContentEntityBase implements ContentEntityInterface {
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {

    // Standard field, used as unique if primary index.
    $fields['id'] = BaseFieldDefinition::create('integer')
      ->setLabel(t('ID'))
      ->setDescription(t('The ID of the MyEntity.'))
      ->setReadOnly(TRUE);

    // Standard field, unique outside of the scope of the current project.
    $fields['name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Name'))
      ->setDescription(t('The name of the MyEntity.'))
      ->setReadOnly(TRUE);

    return $fields;
  }
}
