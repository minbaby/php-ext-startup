<?php
use Kahlan\Cli\CommandLine;

/**@var \Kahlan\Cli\CommandLine $commandLine */
$commandLine = $this->commandLine();
// $commandLine->option('reporter', 'default', 'verbose');
$commandLine->option('ff', 'default', 1);
$commandLine->option('clover', 'default', 'coverage.xml');
