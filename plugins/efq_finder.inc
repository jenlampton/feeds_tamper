<?php

$plugin = array(
  'form' => 'feeds_tamper_efq_finder_form',
  'callback' => 'feeds_tamper_efq_finder_callback',
  'validate' => 'feeds_tamper_efq_finder_validate',
  'name' => 'Entity Field Query finder',
  'multi' => 'loop',
  'category' => 'Other',
);

function feeds_tamper_efq_finder_form($importer, $element_key, array $settings, aray &$form_state) {
  $form = array(
    '#prefix' => '<div id="feeds-tamper-efq-finder-wrapper">',
    '#suffix' => '</div>'
  );

  // Get some info on entities and bundles.
  $info = entity_get_info();
  $entity_types = array();

  foreach ($info as $key => $data) {
    $entity_types[$key] = check_plain($data['label']);
  }

  $entity_type = !empty($form_state['values']['settings']['entity_type']) ? $form_state['values']['settings']['entity_type'] : NULL;
  $entity_type = !$entity_type && !empty($settings['entity_type']) ? $settings['entity_type'] : $entity_type;
  $entity_type = !$entity_type ? key($entity_types) : $entity_type;

  $form['entity_type'] = array(
    '#type' => 'select',
    '#title' => t('Entity type'),
    '#options' => $entity_types,
    '#default_value' => $entity_type,
    '#ajax' => array(
      'callback' => 'feeds_tamper_efq_finder_ajax',
      'wrapper' => 'feeds-tamper-efq-finder-wrapper',
    ),
  );

  $bundles = array();
  if (!empty($info[$entity_type]['bundles'])) {
    foreach ($info[$entity_type]['bundles'] as $bundle => $bundle_info) {
      $bundles[$bundle] = check_plain($bundle_info['label']);
    }
  }
  else {
    $bundles[$entity_type] = $entity_types[$entity_type];
  }

  $bundle = !empty($form_state['values']['settings']['bundle']) ? $form_state['values']['settings']['bundle'] : NULL;
  $bundle = !$bundle && !empty($settings['bundle']) ? $settings['bundle'] : $bundle;

  if (!isset($bundles[$bundle])) {
    $bundle = key($bundles);
  }

  $form['bundle'] = array(
    '#type' => 'select',
    '#title' => t('Bundle'),
    '#options' => $bundles,
    '#default_value' => $bundle,
    '#ajax' => array(
      'callback' => 'feeds_tamper_efq_finder_ajax',
      'wrapper' => 'feeds-tamper-efq-finder-wrapper',
    ),
  );

  $fields = array();
  foreach (field_info_instances($entity_type, $bundle) as $key => $info) {
    $fields[$key] = check_plain($info['label']);
  }

  $field = !empty($form_state['values']['settings']['field']) ? $form_state['values']['settings']['field'] : NULL;
  $field = !$field && !empty($settings['field']) ? $settings['field'] : $field;

  if (!isset($fields[$field])) {
    $field = key($fields);
  }

  $form['field'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#options' => $fields,
    '#default_value' => $field,
    '#ajax' => array(
      'callback' => 'feeds_tamper_efq_finder_ajax',
      'wrapper' => 'feeds-tamper-efq-finder-wrapper',
    ),
  );

  // If a bundle doesn't have any fields, then there won't be any columns.
  if (!$field) {
    return $form;
  }

  $field_info = field_info_field($field);
  $columns = array();
  foreach (array_keys($field_info['columns']) as $column) {
    $columns[$column] = $column;
  }

  $column = !empty($form_state['values']['settings']['column']) ? $form_state['values']['settings']['column'] : NULL;
  $column = !$column && !empty($settings['column']) ? $settings['column'] : $field;

  if (!isset($columns[$column])) {
    $column = key($columns);
  }

  $form['column'] = array(
    '#type' => 'select',
    '#title' => t('Column'),
    '#options' => $columns,
    '#default_value' => $column,
    '#ajax' => array(
      'callback' => 'feeds_tamper_efq_finder_ajax',
      'wrapper' => 'feeds-tamper-efq-finder-wrapper',
    ),
  );

  return $form;
}

function feeds_tamper_efq_finder_validate(array &$settings) {
  // Ensure we can do a full entity query.
  if (empty($settings['column'])) {
    form_set_error('', t("This bundle doesn't have any fields."));
  }
}

function feeds_tamper_efq_finder_ajax(array $form, array &$form_state) {
  return isset($form['plugin']['settings']) ? $form['plugin']['settings'] : $form['settings'];
}

function feeds_tamper_efq_finder_callback($result, $item_key, $element_key, &$field, array $settings) {
  $query = new EntityFieldQuery();

  $query->entityCondition('entity_type', $settings['entity_type']);
  $query->entityCondition('bundle', $settings['bundle']);

  // @todo: Implement choice of behavior when multiple entities are found.
  // Currently we only use the first.
  $query->fieldCondition($settings['field'], $settings['column'], $field, '=');
  $query->range(0, 1);

  $result = $query->execute();

  if (isset($result[$settings['entity_type']])) {
    $field = key($result[$settings['entity_type']]);
  }
}
