<?php
header('Content-Type: text/plain');

echo "CGI OK\n";
echo "Method: " . $_SERVER['REQUEST_METHOD'] . "\n";
echo "Script: " . $_SERVER['SCRIPT_FILENAME'] . "\n";
echo "Query: " . (isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '') . "\n";

echo "Body: ";
$body = file_get_contents('php://input');
echo $body !== false ? $body : '';
echo "\n";
