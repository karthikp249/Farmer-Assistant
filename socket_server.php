<?php
error_reporting(E_ERROR);

/* Allow the script to hang around waiting for connections. */
set_time_limit(0);

/* Turn on implicit output flushing so we see what we're getting
 * as it comes in. */
ob_implicit_flush();

$address = '172.31.41.42';
$port = 9000;
$shouldIrrigate="0";

if (($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
    echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
}

if (socket_bind($sock, $address, $port) === false) {
    echo "socket_bind() failed: reason: " . socket_strerror(socket_last_error($sock)) . "\n";
}

if (socket_listen($sock, 5) === false) {
    echo "socket_listen() failed: reason: " . socket_strerror(socket_last_error($sock)) . "\n";
}

//clients array
$clients = array();

do {
    $read = array();
    $read[] = $sock;
   
    $read = array_merge($read,$clients);
   
    // Set up a blocking call to socket_select
    if(socket_select($read,$write = NULL, $except = NULL, $tv_sec = 5) < 1)
    {
        //    SocketServer::debug("Problem blocking socket_select?");
        continue;
    }
   
    // Handle new Connections
    if (in_array($sock, $read)) {       
       
        if (($msgsock = socket_accept($sock)) === false) {
            echo "socket_accept() failed: reason: " . socket_strerror(socket_last_error($sock)) . "\n";
            break;
        }
        $clients[] = $msgsock;
        $key = array_keys($clients, $msgsock);
        /* Send instructions. */
        /*$msg = "\nWelcome to the PHP Test Server.  \n" .
        "You are the customer number: {$key[0]}\n" .
        "To quit, type 'quit'. To close the server type 'shutdown'.\n";*/
		$shouldIrrigate=file_get_contents("irrigate.txt");
		//$msg="1\n";
		$msg=$shouldIrrigate."\n";
        socket_send($msgsock, $msg, strlen($msg),MSG_EOR);
       
    }
   
    // Handle Input
    foreach ($clients as $key => $client) { // for each client       
        if (in_array($client, $read)) {
            if (false === ($buf = socket_read($client, 2048, PHP_BINARY_READ))) {
                echo "socket_read() failed: reason: " . socket_strerror(socket_last_error($client)) . "\n";
                break 2;
            }
            if (!$buf = trim($buf)) {
                continue;
            }
            if ($buf == 'quit') {
                unset($clients[$key]);
                socket_close($client);
                break;
            }
            if ($buf == 'shutdown') {
                socket_close($client);
                break 2;
            }
            $talkback = "Client {$key}: You said '$buf'.\n";
			
			$string = "/Irrigating:(.*)/";
			preg_match($string, $buf, $datatemp);
			
			if(isset($datatemp[1]))
			{
				if($datatemp[1]=="YES")
				{
					file_put_contents("/var/www/html/irrigate.txt", "1");
				}
				else if($datatemp[1]=="NO")
				{
					file_put_contents("/var/www/html/irrigate.txt", "0");
				}
			}
			
			// Write the contents back to the file
			//$file = "data.txt";
			file_put_contents("/var/www/html/data.txt", $buf);
			
            //socket_send($client, $talkback, strlen($talkback),MSG_EOR);
            /*echo "$buf\n";
			echo $talkback;*/
			
			
			unset($clients[$key]);
            socket_close($client);
        }
       
    }       
} while (true);

socket_close($sock);
?>