#!/usr/bin/php
<?php
header('Content-Type: text/plain');
// Activer l'affichage des erreurs PHP
ini_set('display_errors', 1);
error_reporting(E_ALL);

if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    $reqBody = file_get_contents('php://stdin');
    parse_str($reqBody, $_POST);
        $sign = $_POST['sign'];
        echo "Received sign: " . htmlspecialchars($sign) . "<br>";

        switch ($sign) {
            case 'Aries':
                echo "Bold moves bring rewards; trust instincts today.";
                break;
            case 'Taurus':
                echo "Patience pays off; unexpected gains arrive tonight.";
                break;
            case 'Leo':
                echo "Creativity shines; share your talents, applause follows.";
                break;
            case 'Gemini':
                echo "Conversations spark joy; a surprise message lifts spirits.";
                break;
            case 'Cancer':
                echo "Home comforts heal; embrace quiet moments this week.";
                break;
            case 'Virgo':
                echo "Organize priorities, clarity emerges—trust your meticulous eye.";
                break;
            case 'Libra':
                echo "Harmony restored through compromise; love blooms unexpectedly.";
                break;
            case 'Scorpio':
                echo "Passion ignites; take risks, transformation begins.";
                break;
            case 'Sagittarius':
                echo "Wanderlust calls; say yes to spontaneous journeys.";
                break;
            case 'Capricorn':
                echo "Goals align; persistence opens doors—keep climbing.";
                break;
            case 'Aquarius':
                echo "Ideas electrify; collaborate, innovation thrives.";
                break;
            case 'Pisces':
                echo "Dreams guide; act on intuition, magic unfolds.";
                break;
            default:
                echo "Please enter a valid sign.";
                break;
        }
    } else {
    echo "Request method is not POST.";
}
?>

