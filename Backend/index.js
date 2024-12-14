const express = require('express');
const nodemailer = require('nodemailer');

const app = express();
const appPort = 3008;

const sendMail = async () => {
    console.log('sending mail');

    const transporter = nodemailer.createTransport({
        host: "smtp.gmail.com",
        port: 587,
        ssl: false,
        auth: {
          user: "emailtrigger33@gmail.com",
          pass: "fgkndifdnbwwyftk",
        },
    });

    const mailOptions = {
        from: "emailtrigger33@gmail.com",
        to: "omarsahmoud02@gmail.com",
        subject: "Button triggered!",
        text: "The door is open.",
    };

    try {
        const result = await transporter.sendMail(mailOptions);
        if (!result) {
            console.error("Error sending email: ", result);
            return false;
        }
    } catch(error) {
        console.log('Email sender crashed.');
        return false;
    }

    console.log("Email sent");
    return true;
};

app.get('/trigger', async (req, res) => {
    if(!(await sendMail()))
    {
        res.json({
            status: false,
            message: 'Mail failed to send'
        });

        return;
    }
    
    res.json({
        status: true,
        message: 'Success'
    });
});

app.listen(appPort, () => {
    console.log('listening');
});