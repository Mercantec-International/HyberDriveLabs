// ignore_for_file: must_be_immutable
import 'package:intl/intl.dart';
import 'package:rc_controller/ui/controller/controller_page.dart';
import 'package:flutter/material.dart';

class CustomListItem extends StatelessWidget {
  CustomListItem(
      {super.key,
      required this.id,
      required this.lastEmergency,
      required this.createdAt,
      required this.updateFunc,
      required this.index});
  final String id;
  final DateTime lastEmergency, createdAt;
  final int index;
  void Function() updateFunc;

  @override
  // Custom item to show in list page showing the company, license plate and status of each entry
  Widget build(BuildContext context) {
    return Column(
      children: [
        GestureDetector(
            onTap: () async {
              await Navigator.push(context, MaterialPageRoute(builder: (context) => const ControllerPage()));
            },
            child: SizedBox(
              width: MediaQuery.of(context).size.width,
              child: Container(
                color: Colors.blue,
                padding: const EdgeInsets.all(10),
                child: DefaultTextStyle(
                  style: const TextStyle(
                    color: Colors.white,
                  ),
                  child: Row(
                    children: [
                      SizedBox(
                        width: MediaQuery.of(context).size.width - 70,
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.start,
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Column(
                              mainAxisAlignment: MainAxisAlignment.start,
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                //Title
                                const Text("Id:"),
                                //Content
                                SizedBox(
                                  width: MediaQuery.of(context).size.width - 55,
                                  child: Text(id,
                                    maxLines: 2,
                                    overflow: TextOverflow.ellipsis,
                                    style: const TextStyle(fontSize: 20),
                                  ),
                                )
                              ],
                            ),
                            const SizedBox(height: 10),
                            Column(
                              mainAxisAlignment: MainAxisAlignment.start,
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                const Text("Last emergency:"),
                                Text(
                                  DateFormat('hh:mm:ss - dd-MM-yyyy').format(lastEmergency), maxLines: 1,
                                  overflow: TextOverflow.ellipsis,
                                  style: const TextStyle(fontSize: 20),
                                ),
                              ],
                            ),
                            Column(
                              mainAxisAlignment: MainAxisAlignment.start,
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                const SizedBox(height: 10),
                                const Text('Created at:'),
                                Text(
                                  DateFormat('hh:mm:ss - dd-MM-yyyy').format(createdAt), maxLines: 1,
                                  overflow: TextOverflow.ellipsis,
                                  style: const TextStyle(fontSize: 20),
                                ),
                              ],
                            ),
                          ],
                        ),
                      ),
                      const Spacer(),
                    ],
                  ),
                ),
              ),
            ),
          ),
      ],
    );
  }
}
