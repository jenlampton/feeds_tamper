<?php

/**
 * @file
 * URL Decode text.
 */

$plugin = array(
  'form' => 'feeds_tamper_urldecode_form',
  'callback' => 'feeds_tamper_urldecode_callback',
  'validate' => 'feeds_tamper_urldecode_validate',
  'multi' => 'loop',
  'name' => t('URL Decode'),
  'category' => t('Text'),
);

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_urldecode_form($importer, $element_key, array $settings, array &$form_state) {
  $form = array();
  $form['help']['#markup'] = t('Run values through the <a href="!url">urldecode()</a> function.', array('!url' => 'http://us3.php.net/urldecode'));
  $form['method'] = array(
    '#type' => 'radios',
    '#title' => t('Decode method'),
    '#options' => array(
      'raw' => t('Raw'),
      'legacy' => t('Legacy: Decodes + symbols into spaces.'),
    ),
    '#default_value' => !empty($settings['method']) ? $settings['method'] : 'raw',
  );
  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_urldecode_validate(array &$settings) {
  if ($settings['method'] === 'raw') {
    $settings['function'] = 'rawurldecode';
  }
  else {
    $settings['function'] = 'urldecode';
  }
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function feeds_tamper_urldecode_callback($source, $item_key, $element_key, &$field, array $settings) {
  $field = $settings['function']($field);
}
