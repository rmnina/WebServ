#!/usr/bin/node

const http = require('https');
const apiUrl = 'https://kaamelott.chaudie.re/api/random';

http.get(apiUrl, (res) => {
	let data = '';

	res.on('data', chunk => {
		data += chunk;
	});
	
	res.on('end', () => {
		try {
			const quote = JSON.parse(data).citation.citation;
			const character = JSON.parse(data).citation.infos.personnage;
			console.log(`${quote}   ─ ${character}`);
		} catch (error) {
			console.log("Content-Type: text/plain\n");
			console.log('Error lors de la recuperation de la requete : ', error);
		}
	});
	}).on('error', (error) => {
		console.log("Content-Type: text/plain\n");
		console.log('Error de fetch : ', error);
});
