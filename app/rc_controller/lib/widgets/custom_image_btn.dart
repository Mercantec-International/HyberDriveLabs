import 'package:flutter/material.dart';

class CustomImageBtn extends StatelessWidget {
  const CustomImageBtn({super.key, required this.text, required this.onTap,
    required this.image, required this.updateFunc, required this.gradient});
  final String text;
  final void Function() onTap, updateFunc;
  final Image image;
  final LinearGradient gradient;

  @override
  // Custom button with image and text
  Widget build(BuildContext context) => GestureDetector(
    // Update color
    onTapCancel: updateFunc,
    onTapUp: (details) => updateFunc(),
    onTapDown: (details) => updateFunc(),
    // Run given function when pressed
    onTap: () async => {updateFunc(), updateFunc(), onTap()},
    child: Container(
      padding: const EdgeInsets.all(10),
      height: 60,
      width: 300,
      decoration: BoxDecoration(
        gradient: gradient,
        border: Border.all(),
        borderRadius: BorderRadius.circular(5),
      ),
      child: Row(
        children: [
          //Image
          SizedBox(height: 45, child: image),
          //Text
          const Spacer(),
          Center(child: Text(text, style: const TextStyle(color: Colors.white, fontSize: 22))),
          const Spacer(),
        ],
      ),
    ),
  );
}