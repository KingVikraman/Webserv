<?php
echo "Content-Type: text/html\r\n\r\n";
echo "Method:" . $_SERVER['REQUEST_METHOD'] . "\n";
echo "Query: " . $SERVER['QUERY_STRING'] . "\n";
echo "Body: " . file_get_contents('php://stdin') . "\n";
?>