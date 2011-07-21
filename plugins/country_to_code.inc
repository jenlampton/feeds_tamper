<?php
/**
 * @file
 * Convert a country name to ISO code.
 */

$plugin = array(
  'form'     => 'feeds_tamper_country_to_code_form',
  'callback' => 'feeds_tamper_country_to_code_callback',
  'name'     => 'Country to ISO code',
  'multi'    => 'loop',
  'category' => 'Other',
  'default description' => 'Convert country to ISO code',
);

function feeds_tamper_country_to_code_callback($result, $item_key, $element_key, &$field, $settings) {
  include_once DRUPAL_ROOT . '/includes/locale.inc';
  static $countries;

  if (empty($countries)) {
    $countries = array_flip(country_get_list());
  }

  //Trim whitespace, capitalize and translate country
  $country = t(ucwords(drupal_strtolower(trim($field))));
  if (isset($countries[$country])) {
    $field = $countries[$country];
  }
  else {
    // If country name cannot be found, return nothing.
    $field = '';
  }
}

function feeds_tamper_country_to_code_form($importer, $element_key, $settings) {
  $form = array();
  $form['help'] = array(
    '#markup' => t('Converts this field from a country name string to the two character ISO 3166-1 alpha-2 code.'),
  );
  return $form;
}
