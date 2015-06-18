<?php

/**
 * Implements hook_hosting_site_context_options().
 */
function hosting_scratchpads_hosting_site_context_options(&$task){
  if(module_exists('hosting_scratchpads')){
    $task = &drush_get_context('HOSTING_TASK');
    if($task->ref->type == 'site'){
      $task->context_options['site_data'] = array(
        'client_title' => $task->ref->client_title,
        'fullname' => $task->ref->client_name,
        'institution' => $task->ref->client_institution,
        'site_title' => $task->ref->site_title,
        'client_email' => $task->ref->client_title,
        'country' => $task->ref->client_country,
        'taxonomic_scope' => $task->ref->site_taxonomic_scope
      );
    }
  }
}