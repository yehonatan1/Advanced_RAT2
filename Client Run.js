const express = require('express');
const app = express();


app.get('/', (req, res) => {

    res.sendFile("C:\\Users\\avita\\WebstormProjects\\Advanced_RAT2\\RAT Client.html");
});

app.listen(9087, '0.0.0.0')