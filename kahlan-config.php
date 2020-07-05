<?php


/** @var \Kahlan\Cli\CommandLine $commandLine */
$commandLine = $this->commandLine();
$commandLine->option('ff', 'default', 1);
if (isset($_SERVER['CI']) && $_SERVER['CI'] === true) {
     $commandLine->option('clover', 'default', 'coverage.xml');
     $commandLine->option('reporter', 'default', 'verbose');
}
