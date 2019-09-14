#include <regex>
#include <string>
#include <iostream>

// C++ implementation of python inflect's number_to_words()

static std::string lastN(std::string input, size_t n)
{
    size_t length = input.size();
    return (n > 0 && length > n) ? input.substr(length - n) : "";
}

int main(int argc, char **argv)
{
  std::string input = "42nd";

  if (argc > 1) {
    input = std::string(argv[1]);
  }

  //      # Handle "stylistic" conversions (up to a given threshold)...
  //      if threshold is not None and float(num) > threshold:
  //          spnum = num.split(".", 1)
  //          while comma:
  //              (spnum[0], n) = re.subn(r"(\d)(\d{3}(?:,|\Z))", r"\1,\2", spnum[0])
  //              if n == 0:
  //                  break
  //          try:
  //              return "{}.{}".format(spnum[0], spnum[1])
  //          except IndexError:
  //              return "%s" % spnum[0]

  //      if group < 0 or group > 3:
  //          raise BadChunkingOptionError
  //      nowhite = num.lstrip()
  //      if nowhite[0] == "+":
  //          sign = "plus"
  //      elif nowhite[0] == "-":
  //          sign = "minus"
  //      else:
  //          sign = ""


  // Check ordinal number

  //      myord = num[-2:] in ("st", "nd", "rd", "th")
  //      if myord:
  //          num = num[:-2]

  std::string ord = lastN(input, 2);
  if ((ord.compare("st") == 0) ||
      (ord.compare("nd") == 0) ||
      (ord.compare("rd") == 0) ||
      (ord.compare("th") == 0)) {
    // strip
    input = input.substr(input.size() - 2);
    std::cout << "input = " << input << std::endl;
  }

  //bool finalpoint = false;
  bool is_decimal = false;
  int group = 0;

        //if decimal:
        //    if group != 0:
        //        chunks = num.split(".")
        //    else:
        //        chunks = num.split(".", 1)
        //    if chunks[-1] == "":  # remove blank string if nothing after decimal
        //        chunks = chunks[:-1]
        //        finalpoint = True  # add 'point' to end of output
        //else:
        //    chunks = [num]

  if (is_decimal) {
    if (group != 0) {
      //chunks = num.split(".")
    }  else {

    }
  }

  return EXIT_SUCCESS;

}
