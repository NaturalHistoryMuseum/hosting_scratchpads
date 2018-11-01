<?php

/**
 * The scratchpads service class.
 */
class Provision_Service_scratchpads extends Provision_Service {
  public $service = 'scratchpads';

  /**
   * Add the needed properties to the site context.
   */
  static function subscribe_site($context) {
    $context->setProperty('site_data');
  }
}
