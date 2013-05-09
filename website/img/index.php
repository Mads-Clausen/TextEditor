<html>
    <head>
        <title>Image files</title>
        
        <script src="../css/main.css">
            
        </script>
    </head>
    
    <body>
        <center>
            <h1>Files in /img/</h1>
            <div id="files">
                <?php
                    function ends_with($haystack, $needle)
                    {
                        $length = strlen($needle);
                        if ($length == 0) {
                            return true;
                        }
                    
                        return (substr($haystack, -$length) === $needle);
                    }
                
                    $f_list = scandir(".");
                    
                    foreach($f_list as $f)
                    {
                        if($f != '.' && $f != '..' && !ends_with($f, '.php')) { echo '<div style="padding: 5px; border-top: 1px solid black; width: 30%; text-align: center;"><a style="color: #0098DF; text-decoration: none;" href="' . $f . '">' . $f . '</a></div>' . "\n"; }
                    }
                ?>
            </div>
        </center>
    </body>
</html>