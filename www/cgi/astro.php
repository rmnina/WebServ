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
    	$link_add = 'https://www.miviludes.interieur.gouv.fr/';

        switch ($sign) {
            case 'Aries':
                echo "You will fall in love 20 times per day. Your love life is about to get busy.";
                break;
            case 'Taurus':
                echo "Money is in the air! Gamble everything you have on a parlay tonight, trust the 🔮";
                break;
            case 'Leo':
                echo "Ouch. Sorry, your life just sucks right now. Better to stay inside for the following month.";
                break;
            case 'Gemini':
                echo "7 is your lucky number today! Or was it 11...? Naaaah, go for 7 👌";
                break;
            case 'Cancer':
                echo "Take risks today, leave your job and/or your companion. If you don't have any, ask yourself why.";
                break;
            case 'Virgo':
                echo "Organize priorities, boost your elo in whatever multiplayer game you're playing, taxes will wait.";
                break;
            case 'Libra':
                echo "Open your chakras. Visit <a href ='$link_add' class='spiritual' target='_blank'> www.spiritualiscam.com</a> for the best advices!";
                break;
            case 'Scorpio':
                echo "New day, new you! Get a new identity in South America to start anew!";
                break;
            case 'Sagittarius':
                echo "You have a talent for singing while drinking (or the other way around). Embrace it.";
                break;
            case 'Capricorn':
                echo "Money: you might want to invest into NFTs.";
                break;
            case 'Aquarius':
                echo "You need a change in your life. Get a new tattoo today, you know there's no way you could regret that.";
                break;
            case 'Pisces':
                echo "You might have flabby arms and glassy eyes, just don't believe your mirror.";
                break;
            case 'Rabbit':
                echo "No sweet heart";
                break;
            case 'Tiger':
                echo "R u kinding?";
                break;
            case 'Snake':
                echo "Merci de faire ton boulot Antoine";
                break;
            case 'Goat':
                echo "Merci de faire ton boulot Antoine, au lieu de came a lot devant Monster Hunter";
                break;
            default:
	 	echo "Please enter a real sign. Astrology is serious business";
                break;
        }
    } else {
	 echo "Please enter a real sign. Astrology is serious business!";
}
?>

