require "spec_helper"

RSpec.describe Leven do
  it "has a version number" do
    expect(Leven::VERSION).not_to be nil
  end

  describe ".distance" do
    it "accepts only 2 arguments that can be cast to strings" do
      expect { described_class.distance 134, nil }.to raise_error(TypeError)
      expect { described_class.distance "abc", "cba" }.not_to raise_error
    end

    it "should handle both ascii and UTF-8 characters" do
      expect(described_class.distance "foobar", "buzbar").to eq 3
      expect(described_class.distance "foöbar", "fúzbar").to eq 2
      first_line = "ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ"
      second_line = "ᚳᛖᚪᛚ᛫ᚦᛖᚪᚻ᛫ᛗᚪᚾᚾᚪ᛫ᚷᛖᚻᚹᛦᛚᚳ᛫ᛗᛁᚳᛚᚢᚾ᛫ᚻᛦᛏ᛫ᛞᚫᛚᚪᚾ"
      expect(described_class.distance first_line, second_line).to eq 33

      first_line = "Wealth is a comfort to all men;"
      second_line = "yet must every man bestow it freely,"
      expect(described_class.distance first_line, second_line).to eq 29
    end
  end
end
