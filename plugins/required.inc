<?php

/**
 * @file
 * Make field required. If not present, the feed item will be removed.
 */

$plugin = array(
  'form' => 'feeds_tamper_required_form',
  'callback' => 'feeds_tamper_required_callback',
  'name' => 'Required field',
  'multi' => 'direct',
  'category' => 'Filter',
);

function feeds_tamper_required_form($importer, $element_key, $settings) {
  $form = array();

  $form['help'] = array(
    '#markup' => t('Make this field required. If it is empty, the feed item will not be saved.'),
  );

  $form['invert'] = array(
    '#type' => 'checkbox',
    '#title' => t('Invert filter'),
    '#default_value' => isset($settings['invert']) ? $settings['invert'] : FALSE,
    '#description' => t('Inverting the filter will save items only if the field is empty.'),
  );

  return $form;
}

function feeds_tamper_required_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  if (!empty($settings['invert'])) {
    if (!empty($field)) {
      unset($result->items[$item_key]);
    }

    return;
  }

  if (empty($field)) {
    unset($result->items[$item_key]);
  }
}
