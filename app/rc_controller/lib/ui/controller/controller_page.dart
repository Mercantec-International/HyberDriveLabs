import 'package:flutter/material.dart';

class ControllerPage extends StatelessWidget {
  const ControllerPage({super.key});

  Widget _buildControlButton(String label, VoidCallback onPressed) {
    return SizedBox(
      width: 120,
      height: 120,
      child: ElevatedButton(
        onPressed: onPressed,
        style: ElevatedButton.styleFrom(
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(15),
          ),
          backgroundColor: Colors.black,
          foregroundColor: Colors.white,
          padding: const EdgeInsets.all(16),
        ),
        child: Text(
          label,
          style: const TextStyle(
            fontSize: 16,
            fontWeight: FontWeight.bold,
            color: Colors.white,
          ),
          textAlign: TextAlign.center,
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.black,
        title: const Text(
          'RCXD Controller',
          style: TextStyle(color: Colors.white),
        ),
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          _buildControlButton('Forward', () {
            print('Forward pressed');
          }),
          const SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              _buildControlButton('Left', () {
                print('Left pressed');
              }),
              const SizedBox(width: 16),
              _buildControlButton('Stop', () {
                print('Stop pressed');
              }),
              const SizedBox(width: 16),
              _buildControlButton('Right', () {
                print('Right pressed');
              }),
            ],
          ),
          const SizedBox(height: 16),
          _buildControlButton('Backward', () {
            print('Backward pressed');
          }),
        ],
      ),
      bottomNavigationBar: Container(
        color: Colors.black,
        height: 60,
        child: const Center(
          child: Text(
            '© 2024 RCXD Controller',
            style: TextStyle(
              color: Colors.white,
              fontSize: 14,
            ),
          ),
        ),
      ),
    );
  }
}
