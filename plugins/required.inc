<?php
// $Id$

$plugin = array(
  'form'     => 'feeds_tamper_required_form',
  'callback' => 'feeds_tamper_required_callback',
  'name'     => 'Required field',
  'multi'    => 'direct',
  'category' => 'Filter',
  'description' => 'feeds_tamper_required_description',
  'machine_name' => 'feeds_tamper_required_machine_name',
);

function feeds_tamper_required_form($importer, $element_key, $settings) {
  $form = array();
  $form['help'] = array(
    '#markup' => t('Makes this field required. If it is empty, the feed item will not be saved.'),
  );
  return $form;
}

function feeds_tamper_required_description($settings) {
  return 'If field is empty, do not save item';
}

function feeds_tamper_required_machine_name($settings) {
  return 'make_required';
}

function feeds_tamper_required_callback($result, $item_key, $element_key, &$field, $settings) {
  if (empty($field)) {
    unset($result->items[$item_key]);
  }
}
