import 'dart:sky';

int childNodeCount(parent) {
  int count = 0;
  for (Node node = parent.firstChild; node != null; node = node.nextSibling)
    ++count;
  return count;
}

int childElementCount(parent) {
  int count = 0;
  for (Element element = parent.firstElementChild; element != null; element = element.nextElementSibling)
    ++count;
  return count;
}
