#!/usr/bin/php
<?php

$reqBody = file_get_contents('php://stdin');
parse_str($reqBody, $params);

if (!is_numeric($params['number1']) || !is_numeric($params['number2']))
	$response = "Invalid inputs :/\n";
else {
	$a = (float)$params['number1'];
	$b = (float)$params['number2'];

	$add = $a + $b;
	$sub = $a - $b;
	$mul = $a * $b;
	$div = $b != 0 ? $a / $b : "Invalid";

	$response = "Addition: $add\nSubstraction: $sub\nMultiplication: $mul\nDivision: $div\n";
}



echo "Content-Length: " . strlen($response) . "\r\n";
echo "Content-Type: text/plain\r\n\r\n";
echo $response;

?>
