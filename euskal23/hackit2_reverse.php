<?php
$fin = fopen("data.bin", "rb");
$pos = filesize("data.bin");
$fout = fopen("data_dec.jpg", "wb");
while ($pos > 0) {
	$pos--;
	fseek($fin, $pos);
	$c = ord(fread($fin, 1));
	$c2 = 0;
	for ($i = 0; $i < 8; $i++) {
		$c2 |= (($c >> $i) & 1) << (7 - $i);
	}
	fwrite($fout, chr($c2));
}
