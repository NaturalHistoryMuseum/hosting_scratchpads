<?php

/**
 * Implements hook_hosting_feature().
 */
function hosting_scratchpads_hosting_feature(){
  return array(
    'scratchpads' => array(
      'title' => t('Scratchpads'),
      'description' => t('Adds the required fields for installing a Scratchpad.'),
      'status' => HOSTING_FEATURE_DISABLED,
      'module' => 'hosting_scratchpads',
      'group' => 'optional'
    )
  );
}