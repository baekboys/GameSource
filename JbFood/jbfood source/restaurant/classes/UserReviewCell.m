//
//  UserReviewCell.m
//  Restaurant
//
//  Created by myungsun baek on 11. 2. 11..
//  Copyright 2011 UKG. All rights reserved.
//

#import "UserReviewCell.h"


@implementation UserReviewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code.
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    
    [super setSelected:selected animated:animated];
    
    // Configure the view for the selected state.
}


- (void)dealloc {
    [super dealloc];
}


@end
