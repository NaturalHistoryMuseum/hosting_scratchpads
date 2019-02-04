<?php

/**
 * @file
 * Scratchpads drush module
 * Handles the addition of the site_data to the site context
 * The module name is the same as the feature name (scratchpads)
 */

/**
 * Implements hook_drush_init().
 */
function scratchpads_drush_init() {
  scratchpads_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function scratchpads_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Implements hook_provision_services().
 *
 * Expose the service type this extension defines to provision.
 *
 * @return array
 *   An array with the service type as key,
 *   and the default implementation the value.
 */
function scratchpads_provision_services() {
  scratchpads_provision_register_autoload();
  return array('scratchpads' => NULL);
}

/**
 * Logs site_data from the alias config if present
 */
function scratchpads_validate_site_data() {
  if(isset(d()->site_data)) {
    drush_log('site_data received from the frontend (d()->site_data): ' . print_r(d()->site_data, true));
  } else {
    drush_log('No site_data was stored in the alias file', 'warning');
  }
}

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_scratchpads_post_provision_install() {
  scratchpads_validate_site_data();
}


/**
 * Generate a value for $drupal_hash_salt and append it to Drupal's settings.php file
 * If this is left blank, Drupal generates a salt on the fly each time it needs it,
 * however it does it in a way which is different during the install process vs normal use
 * which breaks the first-time login link.
 * Instead we generate one during site creating, which guarantees the link to work.
 *
 * @param $uri
 *   URI for the site.
 * @param $data
 *   Associative array of data from provisionConfig_drupal_settings::data.
 *
 * @return
 *   Lines to add to the site's settings.php file.
 *
 * @see provisionConfig_drupal_settings
 */
function scratchpads_provision_drupal_config($uri, $data) {
  // This is similar to how Drupal generates the salt - see drupal_get_hash_salt
  $salt = hash('sha256', serialize($data));

  drush_log("Scratchpads: Generating drupal_hash_salt in settings.php");
  return "\$drupal_hash_salt = '$salt';\n";
}
