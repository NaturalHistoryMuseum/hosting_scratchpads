<?php
/**
 * Implements hook_hosting_site_context_options().
 */
function hosting_scratchpads_hosting_site_context_options(&$task){
  if(module_exists('hosting_scratchpads')){
    $task = &drush_get_context('HOSTING_TASK');
    if($task->ref->type == 'site'){
      $task->options['client_name'] = $task->context_options['client_name'] = $task->ref->client_name;
      $task->options['client_email'] = $task->context_options['client_email'] = $task->ref->client_email;
      $task->context_options['site_data'] = array(
        'client_title' => $task->ref->client_title,
        'client_name' => $task->ref->client_name,
        'client_institution' => $task->ref->client_institution,
        'client_email' => $task->ref->client_email,
        'client_country' => $task->ref->client_country,
        'site_title' => $task->ref->site_title,
        'site_taxonomic_scope' => $task->ref->site_taxonomic_scope
      );

      // FIXME: The code above is supposed to add these variables to the alias config file
      // however for some reason this no longer works and the alias file is created without site_data.
      // We can't even create it manually because it gets overwritten with the version missing site_data.
      // Instead we create a separate config file with ".site_data" in its name so that we have access to
      // this information later - it's used in the provison_welcome_mail.inc (scratchpads2 repo)
      // file and I'm not sure where else, if anywhere.
      $config = new Provision_Config_Drushrc_Alias($task->ref->hosting_name.".site_data", array('site_data' => $task->context_options['site_data']));
      $config->write();
    }
  }
}


/**
 * Implements hook_drush_command().
 */
function hosting_scratchpads_drush_command() {
  $commands = array();

  $commands['remote-import-scratchpad'] = array(
    'description' => 'Import a scratchpad from a remote host.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
    'aliases' => array(),
    'arguments' => array(
       'site' => 'The site to import',
     ),
     'options' => array(
       'new_platform' => 'The platform to import into. Defaults to scratchpads master.',
       'remote_host' => 'The platform to import into. Defaults to scratchpads master.',
     ),
     'examples' => array(
      'drush @old_hostmaster import-scratchpad example.myspecies.info' => 'Import example scratchpad into master platform.',
      'drush @old_hostmaster import-scratchpad example.myspecies.info --platform=scratchpads-2.9.3' => 'Import into 2.9.3 platform',
    )
  );

  $commands['remote-import-list-scratchpads'] = array(
     'options' => array(
       'remote_host' => 'The platform to import into. Defaults to scratchpads master.',
     )
  );

  return $commands;
}

function hosting_scratchpads_drush_get_remote_host() {
  $remote_host = drush_get_option('remote_host');

  if(!$remote_host) {
    $remotes = hosting_scratchpads_get_remote_hosts();
    if(count($remotes) < 1) {
      echo "No remote hostmaster installed";
      die;
    }
    if(count($remotes) > 1) {
      echo "Too many hostmasters:";
      print_r($remotes);
      die;
    }

    $remote_host = $remotes[0];
  }

  return $remote_host;
}

function drush_hosting_scratchpads_remote_import_list_scratchpads() {
  $remote_host = hosting_scratchpads_drush_get_remote_host();
  $existing_sites = hosting_scratchpads_get_site_names();

  echo $remote_host;

  $command = provision_backend_invoke($remote_host, 'provision-remote_import_list_sites', array(), array());

  foreach ($command['context']['remote_sites_list'] as $site => $platform) {
    if(!in_array($site, $existing_sites)) {
      echo "$site ($platform)\n";
    }
  }
}

function drush_hosting_scratchpads_remote_import_scratchpad(...$args) {
  $remote_host = hosting_scratchpads_drush_get_remote_host();
  $platform = drush_get_option('new_platform');
  $databases = hosting_scratchpads_database_picker();
  $existing_sites = hosting_scratchpads_get_site_names();

  if(!$platform) {
    echo "No platform, assuming scratchpasmaster";
    $platform = 'platform_scratchpadsmaster';
  }

  $platform = preg_replace("/^(?!@?platform_)/", "@platform_", $platform);

  foreach($args as $site) {
    if(in_array($site, $existing_sites)) {
      echo "$site already exists, skipping...";
      continue;
    }

    provision_backend_invoke(
      $remote_host,
      'provision-remote_import',
      array(),
      array(
        'old_url' => $site,
        'new_url' => $site,
        'platform' => $platform,
        'db_server' => $databases->current()
      )
    );
    $databases->next();
  }
}
