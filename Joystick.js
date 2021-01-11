import { Buffer } from "buffer";
import crc8_buf from "./Crc";
import BluetoothSerial from "react-native-bluetooth-serial-next";
import { ToastAndroid } from "react-native";

export default async function  handleJoystick (x, y) {
  const [horizontal, vertical] = await parseJoystick(x, y);
  let data;
  if (horizontal === 128 && vertical === 128) {
    await sendStop();
    data=await BluetoothSerial.readUntilDelimiter("\r\n");
    console.log("ReceivedStop: " + data);
  }
  else {
    console.log(horizontal, vertical);
    await sendJoystickData(horizontal, vertical);
    data = await BluetoothSerial.readUntilDelimiter("\r\n");
    console.log("ReceivedRunMotors: " + data);
  }
};

async function parseJoystick(x, y) {
  let newX;
  let newY;
  newX = parseInt(x * 128) + 128;
  newX = (newX === 256) ? newX - 1 : newX;
  newY = parseInt(y * 128) + 128;
  newY = (newY === 256) ? newY - 1 : newY;
  return [newX, newY];
}

async function sendStop(){
  return new Promise((resolve, reject) => {
    const buf = Buffer.from([2, 188, 67]);
    BluetoothSerial.write(buf)
      .then(() => {
        console.log('sendStop: ' + buf[0].toString(10)+ buf[1].toString(10) + buf[2].toString(10));
      })
      .catch((err) => {
        ToastAndroid.show(err.message, ToastAndroid.SHORT)
        reject(err);
      });
    resolve("Ok");
  })
}

async function sendJoystickData(h, v) {
  return new Promise((resolve, reject) => {
    const buf = Buffer.from([1, h, v]);
    const crc = new Uint8Array([0, 0]);
    crc[0] = crc8_buf(buf, 0, 3);
    crc[1] = ~crc[0];
    const buf2 = Buffer.from([1, h, v, crc[0], crc[1]]);

    BluetoothSerial.write(buf2)
      .then(() => {
        console.log('sendJoystickData: ' + buf2[0].toString(10)+ buf2[1].toString(10) + buf2[2].toString(10)+ buf2[3].toString(10)+ buf2[4].toString(10));
      })
      .catch((err) => {
        ToastAndroid.show(err.message, ToastAndroid.SHORT)
        reject(err);
      });
    resolve("Ok");
  })
}

