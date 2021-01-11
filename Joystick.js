import { Buffer } from "buffer";
import crc8_buf from "./Crc";
import BluetoothSerial from "react-native-bluetooth-serial-next";
import { ToastAndroid } from "react-native";

export default async function  handleJoystick (x, y, ackNumber) {
  const [horizontal, vertical] = await parseJoystick(x, y);
  let data;
  if (horizontal === 128 && vertical === 128) {
    await sendStop(ackNumber);
    data=await BluetoothSerial.readUntilDelimiter("\r\n");
    console.log("ReceivedStop: " + data);
  }
  else {
    await sendJoystickData(horizontal, vertical, ackNumber);
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

async function sendStop(ackNumber){
  const ackN = new Uint8Array([ackNumber]);
  const buf = new Uint8Array([2, ackN]);
  const crc = new Uint8Array([0, 0]);
  crc[0] = await crc8_buf(buf, 0, 2);
  crc[1] = ~crc[0];
  const buf2 = Buffer.from([2, ackN, crc[0], crc[1]]);
  return new Promise((resolve, reject) => {
    BluetoothSerial.write(buf2)
      .then(() => {
        console.log('sendStop: ' + buf2[0].toString(10) + " "
                                 + buf2[1].toString(10) + " "
                                 + buf2[2].toString(10) + " "
                                 + buf2[3].toString(10));
      })
      .catch((err) => {
        ToastAndroid.show(err.message, ToastAndroid.SHORT)
        reject(err);
      });
    resolve("Ok");
  })
}

async function sendJoystickData(h, v, ackNumber) {
  const ackN = new Uint8Array([ackNumber]);
  const buf = new Uint8Array([1, h, v, ackN]);
  const crc = new Uint8Array([0, 0]);
  crc[0] = await crc8_buf(buf, 0, 4);
  crc[1] = ~crc[0];
  const buf2 = Buffer.from([1, h, v, ackN, crc[0], crc[1]]);
  return new Promise((resolve, reject) => {
    BluetoothSerial.write(buf2)
      .then(() => {
        console.log('sendJoystickData: ' + buf2[0].toString(10) + " "
                                         + buf2[1].toString(10) + " "
                                         + buf2[2].toString(10) + " "
                                         + buf2[3].toString(10) + " "
                                         + buf2[4].toString(10) + " "
                                         + buf2[5].toString(10));
      })
      .catch((err) => {
        ToastAndroid.show(err.message, ToastAndroid.SHORT)
        reject(err);
      });
    resolve("Ok");
  })
}

