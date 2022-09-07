<?php
$slug = filter_input(INPUT_GET, 'slug', FILTER_SANITIZE_ENCODED);
if (!$slug) { http_response_code(404); die(); }

$api = 'https://patchstorage.com/api/alpha';

$search = file_get_contents($api.'/patches?platform=7834&slug='.$slug);
if (!$search) { http_response_code(404); die(); }

$searchJ = json_decode($search, true);
if (!$searchJ) { http_response_code(404); die(); }
if (!$searchJ[0]) { http_response_code(404); die(); }

$patchId = $searchJ[0]['id'];
if (!$patchId) { http_response_code(404); die(); }

$patchDetails = file_get_contents($api.'/patches/'.$patchId);
if (!$patchDetails) { http_response_code(404); die(); }

$patchDetailsJ = json_decode($patchDetails, true);
if (!$patchDetailsJ) { http_response_code(404); die(); }

$patchFiles = $patchDetailsJ['files'];
if (!$patchFiles) { http_response_code(404); die(); }

$patchFileDetails = $patchFiles[0];
if (!$patchFileDetails) { http_response_code(404); die(); }
if (!$patchFileDetails['filename']) { http_response_code(404); die(); }
if (!$patchFileDetails['url']) { http_response_code(404); die(); }

$contents = file_get_contents($patchFileDetails['url']);
if (!$contents) { http_response_code(404); die(); }

header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Disposition: attachment; filename="'.$patchFileDetails['filename'].'"');
header('Expires: 0');
header('Cache-Control: must-revalidate');
header('Pragma: public');
header('Content-Length: ' . strlen($contents));
flush();
die($contents);
?>
