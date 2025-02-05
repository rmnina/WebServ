#!/usr/bin/php
<?php
    file_put_contents("debug.txt", "RAW INPUT: " . file_get_contents("php://input") . "\n", FILE_APPEND);
    file_put_contents("debug.txt", "POST DATA: " . print_r($_POST, true) . "\n", FILE_APPEND);
    if (isset($_POST['sign']) && isset($_POST['sign']) == "Aries")
        echo $_POST['sign'] . "<br/><strong>: Bold moves bring rewards; trust instincts today.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Taurus")
        echo $_POST['sign'] . "<br/><strong>: Patience pays off; unexpected gains arrive tonight.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Gemini")
        echo $_POST['sign'] . "<br/><strong>: Conversations spark joy; a surprise message lifts spirits.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Cancer")
        echo $_POST['sign'] . "<br/><strong>: Home comforts heal; embrace quiet moments this week.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Leo")
        echo $_POST['sign'] . "<br/><strong>: Creativity shines; share your talents, applause follows.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Virgo")
        echo $_POST['sign'] . "<br/><strong>: Organize priorities, clarity emerges—trust your meticulous eye.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Libra")
        echo $_POST['sign'] . "<br/><strong>: Harmony restored through compromise; love blooms unexpectedly.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Scorpio")
        echo $_POST['sign'] . "<br/><strong>: Passion ignites; take risks, transformation begins.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Sagittarius")
        echo $_POST['sign'] . "<br/><strong>: Wanderlust calls; say yes to spontaneous journeys.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Capricorn")
        echo $_POST['sign'] . "<br/><strong>: Goals align; persistence opens doors—keep climbing.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Aquarius")
        echo $_POST['sign'] . "<br/><strong>: Ideas electrify; collaborate, innovation thrives.</strong> ";
    else if (isset($_POST['sign']) && isset($_POST['sign']) == "Pisces")
        echo $_POST['sign'] . "<br/><strong>: Dreams guide; act on intuition, magic unfolds.</strong> ";

    else
        echo "Please enter a real sign. Astrology is serious business";
    //file_put_contents("debug_headers.txt", print_r(getallheaders(), true));
?>